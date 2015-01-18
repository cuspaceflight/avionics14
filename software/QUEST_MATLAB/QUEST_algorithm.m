format long

%%Setup Matrices
MagReference = [0;1;0];
AccelReference = [0;0;-1];


%MagObservation = [0;1;0];
%AccelObservation = [0;0;-1];

degToRad = 2.0 * pi / 360.0;

% (roll, yaw, pitch)
dcm = angle2dcm(35 * degToRad, -37 * degToRad,-180 * degToRad, 'xyz');

MagObservation = dcm*MagReference;
AccelObservation = dcm*AccelReference;

MagObservation = (MagObservation/norm(MagObservation))
AccelObservation = (AccelObservation/norm(AccelObservation))

magA = 0.5;
accelA = 0.5;

%% Compute Quaternion

% This method becomes less accurate (tending to a singularity) as the angle
% about any axis tends to pi (i.e gamma tends to 0)
% This can be dealt by rotating the reference vectors by pi about the
% problematic axis and then rotating the quaternion back at the end when
% gamma is below a certain threshold value
% This has not been implemented here but might be implemented on the rocket

B = accelA*AccelObservation*transpose(AccelReference) + magA*MagObservation*transpose(MagReference)

S = B + transpose(B)

sigma = magA*dot(MagObservation,MagReference) + accelA*dot(AccelObservation,AccelReference)

Z = [(B(2,3)-B(3,2)); (B(3,1)-B(1,3)); (B(1,2)-B(2,1))]

I3 = [1 0 0; 0 1 0; 0 0 1];

deltaCos = dot(MagObservation,AccelObservation)*dot(MagReference,AccelReference) + norm(cross(MagObservation,AccelObservation))*norm(cross(MagReference,AccelReference))

lambda = sqrt(magA^2 + 2*magA*accelA*deltaCos + accelA^2);

Y = (lambda+sigma)*I3-S;
detY = det(Y)
Y = inv(Y);
Y = Y*Z;

Q3 = [Y; 1] / sqrt(norm(Y)*norm(Y) + 1)

[X, Y, Z] = EulerAngles(Q3)



