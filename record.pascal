program example(input, output);
var Books,books : record
  title: array [1..50] of char;
  author: array [1..50] of char;
  subject: array [1..100] of char;
  bookid: integer;
end;
b : integer;
i : integer;
x : real;

procedure bubble;
var
  a:array [1..4] of integer;
  b:array [1..3,2..5] of integer;
  i,j,temp:integer;
begin
  for i:=1 to 4 do read (a[i]);
  for i:=1 to 4 do
  for j:=1 to 4-i do
    if a[j]>a[j+1] then
    begin
      temp:=a[j];
      a[j]:=a[j+1];
      a[j+1]:=temp;
    end;
  for i:=1 to 4 do write (a[i]);
end;



function f(var m,n: integer ; x : char ): integer;
var j : integer;
begin
  j := f(n,m mod n,x);
end;

begin
  books.bookid := 1;
  b := 5;
  bubble;
  while i < 5 do 
    x := x * i;
end.
