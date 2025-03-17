MODULE Square;
VAR
  num, result: INTEGER;

PROCEDURE CalcSquare(x: INTEGER) : INTEGER;
BEGIN
  RETURN x * x
END CalcSquare;

BEGIN
  num := 4;
  result := CalcSquare(num) * CalcSquare(num)
END Square.