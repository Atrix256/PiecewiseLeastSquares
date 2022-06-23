import matplotlib.pyplot as plt
import numpy as np

fitx = [1,3]
fity = [2,3]

constrainx = [2]
constrainy = [4]

fndomain = [1, 3]
fncoeffs = [3, 0.5]  # in ascending order of power, starting with the constant term

title = "Linear fit"

def PolyCoefficients(x, coeffs):
    y = 0
    for i in range(len(coeffs)):
        y += coeffs[i]*x**i
    return y

fnx = np.linspace(fndomain[0], fndomain[1], 100)
plt.plot(fnx, PolyCoefficients(fnx, fncoeffs), label="Polynomial Fit")
plt.scatter(fitx, fity, label= "Fit Points")
plt.scatter(constrainx, constrainy, label= "Constraint Points")

#plt.xlabel('x')
#plt.ylabel('y')
plt.title(title)
plt.legend()

plt.show()
