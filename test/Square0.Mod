MODULE Square;
VAR
  num, result: INTEGER;

PROCEDURE CalcSquare(VAR x: INTEGER);
BEGIN
  result := x * x
END CalcSquare;

BEGIN
  num := 4;
  CalcSquare(num)
END Square.