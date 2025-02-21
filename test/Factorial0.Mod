MODULE Factorial;
VAR
  n, fact, i: INTEGER;
BEGIN
  n := 5;
  fact := 1;
  i := 1;
  WHILE i <= n DO
    fact := fact * i;
    i := i + 1
  END
END Factorial.