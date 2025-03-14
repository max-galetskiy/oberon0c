MODULE Square;
VAR
  num: INTEGER;

PROCEDURE CalcSquare(VAR x: INTEGER) : INTEGER;
BEGIN
  RETURN x * x
END CalcSquare;

BEGIN
  num := 4;
  CalcSquare(num)
END Square.