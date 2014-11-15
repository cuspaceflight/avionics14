format long

%%Setup Matrices
MagReference = [1;0;0];
AccelReference = [0;0;-1];


%90 degree rotation in Z
MagObservation = [0;1;0];
AccelObservation = [0;0;-1];


MagObservation = MagObservation/norm(MagObservation);
AccelObservation = AccelObservation/norm(AccelObservation);

magA = 0.5;
accelA = 0.5;

%%Compute Quaternion

% This method becomes less accurate (tending to a singularity) as the angle
% about any axis tends to pi (i.e gamma tends to 0)
% This can be dealt by rotating the reference vectors by pi about the
% problematic axis and then rotating the quaternion back at the end when
% gamma is below a certain threshold value
% This has not been implemented here but might be implemented on the rocket

B = accelA*AccelObservation*transpose(AccelReference) + magA*MagObservation*transpose(MagReference);


S = B + transpose(B);

sigma = magA*dot(MagObservation,MagReference) + accelA*dot(AccelObservation,AccelReference);

Z = [(B(2,3)-B(3,2)); (B(3,1)-B(1,3)); (B(1,2)-B(2,1))];

I3 = [1 0 0; 0 1 0; 0 0 1];

deltaCos = dot(MagObservation,AccelObservation)*dot(MagReference,AccelReference) + norm(cross(MagObservation,AccelObservation))*norm(cross(MagReference,AccelReference));

lambda = sqrt(magA^2 + 2*magA*accelA*deltaCos + accelA^2);

delta = det(S);


%Hack using symbolic library as there isn't a double adjoint function
AdjointS = double(adjoint(sym(S)));

k = trace(AdjointS);

alpha = lambda*lambda - sigma*sigma + k;

gamma = (lambda+sigma)*alpha - delta

beta = lambda - sigma;

X = (alpha*I3+beta*S + gamma*S*S)*Z;

%Debugging checks
%{
Test64 = 2*sigma*S*S - k*S + delta*I3 - S*S*S
assert(isequal(Test64,zeros(3,3)))

a = sigma^2 - k;
b = sigma^2 + transpose(Z)*Z;
c = delta + transpose(Z)*S*Z;
d = transpose(Z)*(S^2)*Z;

Test70 = lambda^4 - (a+b)*lambda^2 - c*lambda + (a*b+c*sigma-d);
assert(Test70 == 0)
%}


Q2 = 1/sqrt(gamma*gamma + norm(X)*norm(X)) * [gamma;X]

%Verify valid quaternion
Test40 = transpose(Q2)*Q2;
assert(abs(Test40-1) <= 0.000001)