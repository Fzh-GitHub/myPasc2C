program runerror(output);
const char1 = 'w';
var x : real;
    myarray : array [1..2,3..6,1..6]  of  real;
    i : integer;
    l : boolean;

function f(var m,n: integer ; x : char ): integer;
var j : integer;
begin
  x := char1;
  j := f(n,m mod n,char1);
end;


begin
  x := 3.14;
  myarray[2,3,4] := 5.0;
  for i := 1 to 5 do
    if l then x := 5.0
    else i := i * i;
  begin 
    x := (2.0);
  end;
  while i < 5 do 
    x := x * i;
end.