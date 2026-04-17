def is_less_equal(p1, p2):
    """Porządek produktowy na N^n"""
    return all(x <= y for x, y in zip(p1, p2))

def find_minimal_elements(points):
    minima = []
    for p in points:
        # Sprawdź czy p jest zdominowany przez któryś z obecnych minimów
        is_dominated = False
        for m in minima:
            if is_less_equal(m, p):
                is_dominated = True
                break
        
        if not is_dominated:
            # Usuń z minimów te, które p dominuje
            minima = [m for m in minima if not is_less_equal(p, m)]
            minima.append(p)
    return minima

# Przykład dla zbioru A z zadania:
def get_set_A(a, b):
    points = []
    # Szukamy w kwadracie wokół (a,b) o promieniu sqrt(5) ~ 2.23
    for x in range(max(0, a-3), a+4):
        for y in range(max(0, b-3), b+4):
            if (x - a)**2 + (y - b)**2 <= 5:
                points.append((x, y))
    return find_minimal_elements(points)

def get_set_B(c, d, e, f):
    """
    Znajduje elementy minimalne w zbiorze B:
    B = { (x1, x2, x3, x4) in N^4 : (x1-c)^2 + (x2-d)^2 + (x3-e)^2 + (x4-f)^2 < 224 }
    """
    points = []
    # Promień r = sqrt(224) ≈ 14.96. Szukamy w zasięgu 15 jednostek.
    # Używamy max(0, ...) bo dziedziną jest N^4 (liczby naturalne).
    
    r_limit = 15 
    for x1 in range(max(0, c - r_limit), c + r_limit + 1):
        for x2 in range(max(0, d - r_limit), d + r_limit + 1):
            for x3 in range(max(0, e - r_limit), e + r_limit + 1):
                for x4 in range(max(0, f - r_limit), f + r_limit + 1):
                    # Sprawdzenie warunku przynależności do zbioru B
                    if (x1-c)**2 + (x2-d)**2 + (x3-e)**2 + (x4-f)**2 < 224:
                        points.append((x1, x2, x3, x4))
    
    # Zwracamy tylko elementy minimalne w porządku produktowym
    return find_minimal_elements(points)