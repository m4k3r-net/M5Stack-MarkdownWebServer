# Programming tricks #
## how to calculate the factorial ##
### Factorial ###
This is a function defined on the set of nonnegative integers. The name comes from the Latin. factorialis-acting, generating, multiplying; factorial of positive integer n is defined as the product of all positive integers from 1 to n inclusive.

![](https://pp.userapi.com/c841023/v841023943/75e44/km1D49dsqYw.jpg)

> Don't be surprised, but the zero factorial is one

    long double fact(int n) {
    	if (n < 0)
    	return 0;
    	if (n == 0)
    	return 1;
    	else
    	return n * fact(n - 1);
    }