import matplotlib.pyplot as plt
import numpy as np

fits = [
    {
        "title": "",
        "file": "1.png",
        "fitx": [0.000000, 1.000000, 2.000000],
        "fity": [0.000000, 10.000000, 2.000000],
        "fitweight": [1.000000, 1.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [3.000000, 1.000000],
    },
]

# in ascending order of power, starting with the constant term
def PolyCoefficients(x, coeffs):
    y = 0
    for i in range(len(coeffs)):
        y += coeffs[i]*x**i
    return y

for fit in fits:
    fig = plt.figure()

    minx = min(fit["fitx"])
    maxx = max(fit["fitx"])
    if len(fit["constrainx"]) > 0:
        minx = min(minx, min(fit["constrainx"]))
        maxx = max(maxx, max(fit["constrainx"]))
    
    fnx = np.linspace(minx, maxx, 100)
    plt.plot(fnx, PolyCoefficients(fnx, fit["fncoeffs"]), label="Polynomial Fit")
    plt.scatter(fit["fitx"], fit["fity"], label= "Fit Points")

    if len(fit["constrainx"]) > 0:
        plt.scatter(fit["constrainx"], fit["constrainy"], label= "Constraint Points")

    #plt.xlabel('x')
    #plt.ylabel('y')
    plt.title(fit["title"])
    plt.legend()

    fig.savefig(fit["file"])

