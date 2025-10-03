def myers_diff(a: str, b: str):
    n, m = len(a), len(b)
    maxd = n + m
    v = {1: 0}  # диагональ → координата x
    trace = []

    for d in range(maxd + 1):
        trace.append(v.copy())
        for k in range(-d, d + 1, 2):
            # выбираем направление
            if k == -d or (k != d and v.get(k - 1, 0) < v.get(k + 1, 0)):
                x = v.get(k + 1, 0)      # вниз (вставка)
            else:
                x = v.get(k - 1, 0) + 1  # вправо (удаление)
            y = x - k

            # сдвигаем по диагонали (совпадения)
            while x < n and y < m and a[x] == b[y]:
                x += 1
                y += 1

            v[k] = x
            if x >= n and y >= m:
                return _backtrack(trace, a, b, n, m)


def _backtrack(trace, a, b, n, m):
    """Восстановление пути по сохранённым диагоналям."""
    res = []
    x, y = n, m

    for d in range(len(trace) - 1, -1, -1):
        v = trace[d]
        k = x - y

        if k == -d or (k != d and v.get(k - 1, 0) < v.get(k + 1, 0)):
            prev_k = k + 1
        else:
            prev_k = k - 1

        prev_x = v[prev_k]
        prev_y = prev_x - prev_k

        while x > prev_x and y > prev_y:
            res.append((" ", a[x - 1]))  # совпадение
            x -= 1
            y -= 1

        if d > 0:
            if x == prev_x:
                res.append(("+", b[y - 1]))  # вставка
                y -= 1
            else:
                res.append(("-", a[x - 1]))  # удаление
                x -= 1

    return res[::-1]


print(myers_diff('AABCAB', 'ABBCBAB'))