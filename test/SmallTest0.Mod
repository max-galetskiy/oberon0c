MODULE SmallTest0;

VAR
    a : INTEGER;
    b : INTEGER;
    c : INTEGER;
    d : INTEGER;
    e : INTEGER;
    f : INTEGER;
    g : INTEGER;
    h : INTEGER;

PROCEDURE func(x, y : INTEGER);
VAR j : INTEGER;
BEGIN
    j := 0;
    WHILE j < x + y DO
        j := j + 1
    END
END func;

BEGIN
    IF (a <= b) THEN a := 0 ELSIF (c = d) THEN a := 0 ELSIF (g < h) THEN a := 0 ELSIF (g > 50) THEN a := 0 ELSIF (g < 50) THEN a := 0 ELSE a := 0 END;
    WHILE (g < h) DO
        g := g - 1
    END;
    func(c,d)
END SmallTest0.

