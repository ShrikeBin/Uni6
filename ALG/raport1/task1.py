import math

class GaussianInt:
    def __init__(self, re, im):
        self.re = int(re)
        self.im = int(im)

    def __add__(self, other):
        return GaussianInt(self.re + other.re, self.im + other.im)

    def __sub__(self, other):
        return GaussianInt(self.re - other.re, self.im - other.im)

    def __mul__(self, other):
        return GaussianInt(self.re * other.re - self.im * other.im,
                           self.re * other.im + self.im * other.re)

    def conjugate(self):
        return GaussianInt(self.re, -self.im)

    def norm(self):
        return self.re**2 + self.im**2

    def __repr__(self):
        sign = "+" if self.im >= 0 else "-"
        return f"({self.re} {sign} {abs(self.im)}i)"

    @staticmethod
    def divide(z, w):
        den = w.norm()
        if den == 0: raise ZeroDivisionError()
        num = z * w.conjugate()
        q_re = round(num.re / den)
        q_im = round(num.im / den)
        q = GaussianInt(q_re, q_im)
        r = z - (w * q)
        return q, r

def gcd_gauss(a, b):
    while b.norm() != 0:
        a, b = b, GaussianInt.divide(a, b)[1]
    return a

def gcd_list(nums):
    if not nums: return GaussianInt(0, 0)
    res = nums[0]
    for x in nums[1:]:
        res = gcd_gauss(res, x)
    return res