class Polynomial:
    def __init__(self, coeffs):
        # coeffs to słownik {potęga: współczynnik}, np. {3: 1, 0: -1} dla x^3 - 1
        self.data = {k: v for k, v in coeffs.items() if v != 0}

    def degree(self):
        return max(self.data.keys()) if self.data else -1

    def leading_coeff(self):
        return self.data[self.degree()] if self.data else 0

    def __add__(self, other):
        res = self.data.copy()
        for k, v in other.data.items():
            res[k] = res.get(k, 0) + v
        return Polynomial(res)

    def __mul__(self, other):
        res = {}
        for k1, v1 in self.data.items():
            for k2, v2 in other.data.items():
                deg = k1 + k2
                res[deg] = res.get(deg, 0) + v1 * v2
        return Polynomial(res)

    @staticmethod
    def div_mod(f, g):
        q = {}
        r = f.data.copy()
        while r and max(r.keys()) >= g.degree():
            deg_diff = max(r.keys()) - g.degree()
            coeff = r[max(r.keys())] / g.leading_coeff()
            q[deg_diff] = coeff
            # r = r - (coeff * x^deg_diff) * g
            for k, v in g.data.items():
                new_deg = k + deg_diff
                r[new_deg] = r.get(new_deg, 0) - coeff * v
                if abs(r[new_deg]) < 1e-9: del r[new_deg]
        return Polynomial(q), Polynomial(r)

def xgcd(a, b):
    old_r, r = a, b
    old_s, s = Polynomial({0: 1}), Polynomial({})
    old_t, t = Polynomial({}), Polynomial({0: 1})
    
    while r.degree() != -1:
        q, res = Polynomial.div_mod(old_r, r)
        old_r, r = r, res
        old_s, s = s, old_s + (Polynomial({0: -1}) * q * s)
        old_t, t = t, old_t + (Polynomial({0: -1}) * q * t)
    return old_r, old_s, old_t