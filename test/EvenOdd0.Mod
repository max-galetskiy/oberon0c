MODULE EvenOdd;
VAR
  num: INTEGER;
BEGIN
  num := 7;
  IF (num MOD 2) = 0 THEN
    OUT.String(Even)
  ELSE
    OUT.String(Odd)
  END
END EvenOdd.