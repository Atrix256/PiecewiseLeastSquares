import matplotlib.pyplot as plt
import numpy as np

fits = [
    {
        "title": "Linear fit",
        "fitx": [1,3],
        "fity": [2,3],
        "constrainx": [2],
        "constrainy": [4],
        "fncoeffs": [3, 0.5],
    }
]

# in ascending order of power, starting with the constant term
def PolyCoefficients(x, coeffs):
    y = 0
    for i in range(len(coeffs)):
        y += coeffs[i]*x**i
    return y

for fit in fits:
    plt.figure()

    minx = min(min(fit["fitx"]), min(fit["constrainx"]))
    maxx = max(max(fit["fitx"]), max(fit["constrainx"]))
    
    fnx = np.linspace(minx, maxx, 100)
    plt.plot(fnx, PolyCoefficients(fnx, fit["fncoeffs"]), label="Polynomial Fit")
    plt.scatter(fit["fitx"], fit["fity"], label= "Fit Points")

    if len(fit["constrainx"]) > 0:
        plt.scatter(fit["constrainx"], fit["constrainy"], label= "Constraint Points")

    #plt.xlabel('x')
    #plt.ylabel('y')
    plt.title(fit["title"])
    plt.legend()

    plt.show()
