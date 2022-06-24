import matplotlib.pyplot as plt
import numpy as np

fits = [
    {
        "title": "Linear, Weight=0.0",
        "file": "linweight1",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 0.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [1.500000, 0.500000],
        "ylim":[0, 5],
    },
    {
        "title": "Linear, Weight=1.0",
        "file": "linweight2",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [2.000000, 0.500000],
        "ylim":[0, 5],
    },
    {
        "title": "Linear, Weight=2.0",
        "file": "linweight3",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 2.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [2.250000, 0.500000],
        "ylim":[0, 5],
    },
    {
        "title": "Linear, Weight=4.0",
        "file": "linweight4",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 4.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [2.500000, 0.500000],
        "ylim":[0, 5],
    },
    {
        "title": "Linear, Weight=100.0",
        "file": "linweight5",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 100.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [2.970603, 0.499992],
        "ylim":[0, 5],
    },
    {
        "title": "Linear, Constraint",
        "file": "linweight6",
        "fitx": [1.000000, 3.000000],
        "fity": [2.000000, 3.000000],
        "fitweight": [1.000000, 1.000000],
        "constrainx": [2.000000],
        "constrainy": [4.000000],
        "fncoeffs": [3.000000, 0.500000],
        "ylim":[0, 5],
    }
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

    plt.ylim(fit["ylim"])

    if len(fit["constrainx"]) > 0:
        plt.scatter(fit["constrainx"], fit["constrainy"], label= "Constraint Points")

    #plt.xlabel('x')
    #plt.ylabel('y')
    plt.title(fit["title"])
    plt.legend()

    fig.savefig(fit["file"])

