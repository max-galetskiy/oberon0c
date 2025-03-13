MODULE SCE;

VAR a,b,c,d,e,f,result : INTEGER;

BEGIN
    IF((a < 2) OR (b > 0)) THEN f := 12 ELSE f := 13 END;

    IF ((a < 3) & (b < 30) & (c < 60))       THEN result := 0
    ELSIF ((c < 5) OR (b < 20) OR (a > 900)) THEN result := 1
    ELSE                                    result := 2
    END

END SCE.