#include <math.h>
#include <stdlib.h>

double RVuniform(void)
{
	return ((random() + 0.5) / (1.0 + RAND_MAX));
}

double RVexp(void)
{
	return (-log(RVuniform()));
}

double RVnormal(void)
{
	static int s = 0;
	static double r, t;

	if ((s = 1 - s)) {
		r = sqrt(2 * RVexp());
		t = 2 * M_PI * RVuniform();
		return (r * cos(t));
	} else
		return (r * sin(t));
}

void RVprob(double *p, int n)
{
        double s = 1, t;

        if (n <= 0)
                return;

        while (--n > 0) {
                t = exp(-RVexp()/n);
                p[n] = (1-t)*s;
                s *= t;
        }
        p[0] = s;
}
