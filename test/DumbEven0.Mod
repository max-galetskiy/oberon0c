MODULE DumbEven;

    VAR
        num, i: INTEGER; is_even : BOOLEAN;

    BEGIN
        num := 7;
        i := 1;
        is_even := FALSE;

        WHILE (i < num) DO
            is_even := ~ is_even;
            i := i + 1
        END

    END DumbEven.