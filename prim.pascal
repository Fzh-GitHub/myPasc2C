program bubble(input,output);
var
a:array [1..4] of integer;
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
end.