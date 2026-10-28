import math


def f(x):
    return 4 * x**5 + 4 * x - math.exp(-2 * x**3)


def df(x):
    return 20 * x**3 + 4 + 6 * x**2 * math.exp(-2 * x**3)


def fikspunkt_f(x):
    return 4 + 1 / 3 * math.sin(x)


x_0 = 1


# fikspunkt

for _ in range(10):
    x_0 = x_0 - f(x_0) / df(x_0)
    print(x_0)

print(x_0)
