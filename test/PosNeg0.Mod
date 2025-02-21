MODULE PosNegZero;
VAR
  num: INTEGER;
BEGIN
  num := -4;
  IF num > 0 THEN
    OUT.String(Positive)
  ELSIF num < 0 THEN
    OUT.String(Negative)
  ELSE
    OUT.String(Zero)
  END
END PosNegZero.