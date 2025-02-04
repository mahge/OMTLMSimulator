#include <limits>
#include "portable_isnan.h"

#include "double33.h"
#include "double33s.h"
#include "double3.h"

//////////////////////////////////////////////////////////////
//
// .SS Constants
//
// These are the defined constants
//


const double33 zerodouble33 = double33(0.0);

const double33 Idouble33 = double33(1.0, 0.0, 0.0,
				    0.0, 1.0, 0.0,
				    0.0, 0.0, 1.0);
// A identity matrix 




// Non-inlined functions.
double33::double33(const double a11, const double a12, const double a13,
		   const double a21, const double a22, const double a23,
		   const double a31, const double a32, const double a33)
{
    x[0] = a11; x[1] = a12; x[2] = a13;
    x[3] = a21; x[4] = a22; x[5] = a23;
    x[6] = a31; x[7] = a32; x[8] = a33;
}

double33::double33(const double33& val)
{
    x[0] = val.x[0]; x[1] = val.x[1]; x[2] = val.x[2];
    x[3] = val.x[3]; x[4] = val.x[4]; x[5] = val.x[5];
    x[6] = val.x[6]; x[7] = val.x[7]; x[8] = val.x[8];
}

double33& double33::operator=(const double33& m) 
{
    if (this != &m) {
	x[0] = m.x[0]; x[1] = m.x[1]; x[2] = m.x[2];
	x[3] = m.x[3]; x[4] = m.x[4]; x[5] = m.x[5];
	x[6] = m.x[6]; x[7] = m.x[7]; x[8] = m.x[8];
    }
    return *this;
}



double33& double33::operator*=(const double33& b)
{
    double a11 = x[0]*b.x[0];
    a11 += x[1]*b.x[3];
    a11 += x[2]*b.x[6];

    double a12 = x[0]*b.x[1];
    a12 += x[1]*b.x[4];
    a12 += x[2]*b.x[7];

    double a13 = x[0]*b.x[2];
    a13 += x[1]*b.x[5];
    a13 += x[2]*b.x[8];

    double a21 = x[3]*b.x[0];
    a21 += x[4]*b.x[3];
    a21 += x[5]*b.x[6];

    double a22 = x[3]*b.x[1];
    a22 += x[4]*b.x[4];
    a22 += x[5]*b.x[7];

    double a23 = x[3]*b.x[2];
    a23 += x[4]*b.x[5];
    a23 += x[5]*b.x[8];

    double a31 = x[6]*b.x[0];
    a31 += x[7]*b.x[3];
    a31 += x[8]*b.x[6];

    double a32 = x[6]*b.x[1];
    a32 += x[7]*b.x[4];
    a32 += x[8]*b.x[7];

    double a33 = x[6]*b.x[2];
    a33 += x[7]*b.x[5];
    a33 += x[8]*b.x[8];

    x[0] = a11; x[1] = a12;  x[2] = a13;
    x[3] = a21; x[4] = a22;  x[5] = a23;
    x[6] = a31; x[7] = a32;  x[8] = a33;

    return *this;
}	

void double33::Set(const double a11, const double a12, const double a13,
		   const double a21, const double a22, const double a23,
		   const double a31, const double a32, const double a33)
{
    x[0] = a11; x[1] = a12; x[2] = a13;
    x[3] = a21; x[4] = a22; x[5] = a23;
    x[6] = a31; x[7] = a32; x[8] = a33;
}


void double33::Get(double& a11,  double& a12,  double& a13,
		   double& a21,  double& a22,  double& a23,
		   double& a31,  double& a32,  double& a33) const
{
    a11 = x[0]; a12 = x[1]; a13 = x[2];
    a21 = x[3]; a22 = x[4]; a23 = x[5];
    a31 = x[6]; a32 = x[7]; a33 = x[8];
}


void double33::SetExpMatrix(const double3& w, const double t)
{
    const long double norm = w.norm2();
    
    double33 R(Idouble33);
    if (norm > std::numeric_limits<double>::epsilon()) {
    
        // W(1,1) =   0;   W(1,2) = -w(3); W(1,3) =  w(2);
        // W(2,1) =  w(3); W(2,2) =   0;   W(2,3) = -w(1);
        // W(3,1) = -w(2); W(3,2) =  w(1); W(3,3) =   0;
        
        // This is more efficient that lot of index operations.
        double w1, w2, w3;
        w.Get(w1, w2, w3);

        double33 W( 0.0, -w3,   w2,
                    w3,   0.0, -w1,
                   -w2,   w1,   0.0);

        // Intermediate variables to avoid multiple matrix divisions.
        long double a = norm / sin(norm*t);
        long double b = (norm *norm) / (1.0-cos(norm*t));
        R += W/a;
        R += (W*W)/b;
    }    
    
    // Set this matrix to be transpose of R, i.e.,  operator=( R.T() );
    x[0] = R(1,1); x[3] = R(1,2); x[6] = R(1,3);
    x[1] = R(2,1); x[4] = R(2,2); x[7] = R(2,3);
    x[2] = R(3,1); x[5] = R(3,2); x[8] = R(3,3);
}

// Compute SVD of the 3x3 matrix: A = U S Transpose(V), where U & V are unary (rotation) matrices
// and S is diagonal (singular values)
void double33::calc_svd(double3 U[3], double3& S, double3 V[3])
{
    double ata11 = x[0]*x[0] + x[3]*x[3] + x[6]*x[6];
    double ata12 = x[0]*x[1] + x[3]*x[4] + x[6]*x[7];
    double ata13 = x[0]*x[2] + x[3]*x[5] + x[6]*x[8];

    double ata22 = x[1]*x[1] + x[4]*x[4] + x[7]*x[7];
    double ata23 = x[1]*x[2] + x[4]*x[5] + x[7]*x[8];

    double ata33 = x[2]*x[2] + x[5]*x[5] + x[8]*x[8];

    double33s AtA(ata11,ata22,ata33,ata12,ata23,ata13);

    double3 evAtA;
    AtA.calc_eigensystemScaled(evAtA, V);

    for(int i = 1; i <= 3; i++) {
        double ev = evAtA(i);
        if(ev < 0)
            ev = 0; // this is pure numerics. Negative is not possible
        else
            ev = sqrt(ev);
        S(i) = ev;
        if(ev > 1e-30 || ev < -1e-30) {
            U[i-1] = (*this) * V[i-1];
            U[i-1] /= ev;
        }
        else {
            U[i-1] = V[i-1];
        }
    }
}

// Compute rotation matrix U from polar decomposition of the 3x3 matrix:
// A = P U = U P'
void double33::calc_polar_rotation(double33& U)
{
    double3 W[3], V[3];
    double3 S;
    calc_svd(W,S,V);
    U = OuterProduct(W[0],V[0]);
    U += OuterProduct(W[1],V[1]);
    U += OuterProduct(W[2],V[2]);
}

// Compute left polar decomposition of the 3x3 matrix: A = P U,
// P i positive semidefinite and U is unary (rotation) matrix
void double33::calc_left_polar(double33& P, double33& U)
{
    double3 W[3], V[3];
    double3 S;
    calc_svd(W,S,V);
    U = OuterProduct(W[0],V[0]);
    U += OuterProduct(W[1],V[1]);
    U += OuterProduct(W[2],V[2]);

    P = OuterProduct(W[0],W[0]) * S(1);
    P += OuterProduct(W[1],W[1]) * S(2);
    P += OuterProduct(W[2],W[2]) * S(3);
}

// Compute right polar decomposition of the 3x3 matrix: A = U P,
// P i positive semidefinite and U is unary (rotation) matrix
void double33::calc_right_polar(double33& U, double33& P)
{
    double3 W[3], V[3];
    double3 S;
    calc_svd(W,S,V);
    U = OuterProduct(W[0],V[0]);
    U += OuterProduct(W[1],V[1]);
    U += OuterProduct(W[2],V[2]);

    P = OuterProduct(V[0],V[0]) * S(1);
    P += OuterProduct(V[1],V[1]) * S(2);
    P += OuterProduct(V[2],V[2]) * S(3);
}




bool double33::isNan() const
{
    bool tmp = portable_isnan(x[0]) || portable_isnan(x[1]) || portable_isnan(x[2]) ||
        portable_isnan(x[3]) || portable_isnan(x[4]) || portable_isnan(x[5]) ||
        portable_isnan(x[6]) || portable_isnan(x[7]) || portable_isnan(x[8]);

    return tmp;
}


std::ostream& operator<<(std::ostream& o, const double33& M)
{
    return o << "( " << M.x[0] << ' ' << M.x[1] << ' ' << M.x[2] << "\n"
         << M.x[3] << ' ' << M.x[4] << ' ' << M.x[5] << "\n"
         << M.x[6] << ' ' << M.x[7] << ' ' << M.x[8] << " )";
}





Bstring ToStr(const double33& val)
{
    Bstring res;
    res = "  ( " + ToStr(val.x[0]) + " " + ToStr(val.x[1]) + " " + ToStr(val.x[2]) + "  ";
    res+= "  " + ToStr(val.x[3]) + " " + ToStr(val.x[4]) + " " + ToStr(val.x[5]) + "  ";
    res+= "  " + ToStr(val.x[6]) + " " + ToStr(val.x[7]) + " " + ToStr(val.x[8]) + " )";
    return res;
}

const double33 Max(const double33& a,const double33& b)
{
    return double33(a.x[0]>b.x[0]?a.x[0]:b.x[0],
                    a.x[1]>b.x[1]?a.x[1]:b.x[1],
                    a.x[2]>b.x[2]?a.x[2]:b.x[2],
                    a.x[3]>b.x[3]?a.x[3]:b.x[3],
                    a.x[4]>b.x[4]?a.x[4]:b.x[4],
                    a.x[5]>b.x[5]?a.x[5]:b.x[5],
                    a.x[6]>b.x[6]?a.x[6]:b.x[6],
                    a.x[7]>b.x[7]?a.x[7]:b.x[7],
                    a.x[8]>b.x[8]?a.x[8]:b.x[8]);
}

const double33 Min(const double33& a,const double33& b)
{
    return double33(a.x[0]<b.x[0]?a.x[0]:b.x[0],
                    a.x[1]<b.x[1]?a.x[1]:b.x[1],
                    a.x[2]<b.x[2]?a.x[2]:b.x[2],
                    a.x[3]<b.x[3]?a.x[3]:b.x[3],
                    a.x[4]<b.x[4]?a.x[4]:b.x[4],
                    a.x[5]<b.x[5]?a.x[5]:b.x[5],
                    a.x[6]<b.x[6]?a.x[6]:b.x[6],
                    a.x[7]<b.x[7]?a.x[7]:b.x[7],
                    a.x[8]<b.x[8]?a.x[8]:b.x[8]);
}

const double33 fabs(const double33& a)
{
    return double33(fabs(a.x[0]),
                    fabs(a.x[1]),
                    fabs(a.x[2]),
                    fabs(a.x[3]),
                    fabs(a.x[4]),
                    fabs(a.x[5]),
                    fabs(a.x[6]),
                    fabs(a.x[7]),
                    fabs(a.x[8]));
}

