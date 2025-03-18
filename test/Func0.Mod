MODULE Func0;

    TYPE
        FLOATS = ARRAY 20 OF REAL;

    VAR
        f : FLOATS;
        i: INTEGER;
    BEGIN
        i := 1;
        WHILE (i <= 20) DO
            f[i] := 1.0 / 10.0
        END
    END Func0.