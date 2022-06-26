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
        "fncoeffs": [[1.500000, 0.500000]],
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
        "fncoeffs": [[2.000000, 0.500000]],
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
        "fncoeffs": [[2.250000, 0.500000]],
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
        "fncoeffs": [[2.500000, 0.500000]],
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
        "fncoeffs": [[2.970603, 0.499992]],
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
        "fncoeffs": [[3.000000, 0.500000]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic, Weight=0.0",
        "file": "quadweight1",
        "fitx": [1.000000, 2.000000, 2.500000, 3.000000],
        "fity": [2.000000, 4.000000, 1.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 0.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[-3.000031, 6.500039, -1.500010]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic, Weight=1.0",
        "file": "quadweight2",
        "fitx": [1.000000, 2.000000, 2.500000, 3.000000],
        "fity": [2.000000, 4.000000, 1.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 1.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[0.763663, 1.899970, -0.454538]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic, Weight=2.0",
        "file": "quadweight3",
        "fitx": [1.000000, 2.000000, 2.500000, 3.000000],
        "fity": [2.000000, 4.000000, 1.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 2.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[2.307684, 0.012830, -0.025643]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic, Weight=4.0",
        "file": "quadweight4",
        "fitx": [1.000000, 2.000000, 2.500000, 3.000000],
        "fity": [2.000000, 4.000000, 1.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 4.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[3.677403, -1.661271, 0.354834]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic, weight=100.0",
        "file": "quadweight5",
        "fitx": [1.000000, 2.000000, 2.500000, 3.000000],
        "fity": [2.000000, 4.000000, 1.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 100.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[5.877249, -4.349907, 0.965915]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic, Constraint",
        "file": "quadweight6",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 1.000000],
        "constrainx": [2.500000],
        "constrainy": [1.000000],
        "fncoeffs": [[6.000002, -4.500001, 1.000000]],
        "ylim":[0, 5],
    },
    {
        "title": "Quadratic Derivative Constraint",
        "file": "quadderive",
        "fitx": [1.000000, 2.000000, 3.000000],
        "fity": [2.000000, 4.000000, 3.000000],
        "fitweight": [1.000000, 1.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[-5.538460, 9.384618, -2.192309]],
        "ylim":[0, 5],
    },
    {
        "title": "Piecewise Linear, no continuity",
        "file": "plin1",
        "fitx": [1.000000, 2.000000, 3.000000, 4.000000, 5.000000, 6.000000],
        "fity": [3.000000, 4.000000, 2.000000, 6.000000, 7.000000, 5.000000],
        "fitweight": [1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000],
        "constrainx": [],
        "constrainy": [],
        "fncoeffs": [[4.000000, -0.500000], [8.500012, -0.500002]],
        "fnxlims":[[1,3],[4,6]],
        "ylim":[0, 8],
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
    
    for i in range(len(fit["fncoeffs"])):
        if "fnxlims" in fit:
            minx = fit["fnxlims"][i][0]
            maxx = fit["fnxlims"][i][1]
        fnx = np.linspace(minx, maxx, 100)
        plt.plot(fnx, PolyCoefficients(fnx, fit["fncoeffs"][i]), label="Polynomial Fit " + str(i))
    plt.scatter(fit["fitx"], fit["fity"], label= "Fit Points")

    plt.ylim(fit["ylim"])

    if len(fit["constrainx"]) > 0:
        plt.scatter(fit["constrainx"], fit["constrainy"], label= "Constraint Points")

    title = fit["title"] + "\ny = f(x) = "
    for i in range(len(fit["fncoeffs"])):
        if i > 1:
            title = title + " + "
            title = title + str(fit["fncoeffs"][i]) + "*x^" + str(i)
        elif i > 0:
            title = title + " + "
            title = title + str(fit["fncoeffs"][i]) + "*x"
        else:
            title = title + str(fit["fncoeffs"][i])

    #plt.xlabel('x')
    #plt.ylabel('y')
    plt.title(title)
    plt.legend()

    fig.savefig(fit["file"])

