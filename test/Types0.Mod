MODULE Type0;

    TYPE
        INT = INTEGER;
        CHARACTER = CHAR;
        INTARRAY = ARRAY 10 OF INTEGER;
        MYSTRING = ARRAY 16 OF CHAR;
        student = RECORD name,title : MYSTRING; INITIAL : ARRAY 2 OF CHAR; IQ : INT; GradeAverage : REAL END;
        student_pairs = RECORD id : INTEGER; left, right : POINTER TO student END;
    VAR
        a,b,c : INTEGER;
        x,y,z : INT;
        u,v,w : REAL;
        alph,rom,numer : CHAR;
        name : MYSTRING;
        id_a,id_b : STRING;
        b1,b2 : BOOLEAN;
        nested_arr : ARRAY 10 OF ARRAY 10 OF INTEGER;
        p,q : POINTER TO INTEGER;
        super_p : POINTER TO POINTER TO INTEGER;

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
    id_b := "MX-001";
    b1 := TRUE;
    b2 := FALSE OR FALSE;
    p  := NIL;
    q  := p
END Type0.