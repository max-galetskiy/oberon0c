MODULE Type0;

    TYPE
        INT = INTEGER;
        CHARACTER = CHAR;
        INTARRAY = ARRAY 10 OF INTEGER;
        MYSTRING = ARRAY 16 OF CHAR;
    VAR
        a,b,c : INTEGER;
        x,y,z : INT;
        u,v,w : REAL;
        alph,rom,numer : CHAR;
        name : MYSTRING;
        id_a,id_b : STRING;
        nested_arr : ARRAY 10 OF ARRAY 10 OF INTEGER;
        student : RECORD name,title : MYSTRING; INITIAL : ARRAY 2 OF CHAR; IQ : INT; GradeAverage : REAL END;


    PROCEDURE FirstLetter(s: MYSTRING) : CHAR;
        BEGIN
        RETURN s[0]
    END FirstLetter;

BEGIN
    a := 5;
    rom := "i";
    u := 15.0058;
    id_a := "MX-001";
    id_b := "AAAA";
    id_b := "MX-001"
END Type0.