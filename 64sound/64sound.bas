20 for a=1 to 28:poke 54272+a,0:next
30 poke 54272+24, 15
40 poke 54272+1, 100
50 poke 54272+6, 15*16+9
60 poke 54272+8, 70
70 poke 54272+13, 10*16+9
190 open 5,2,3,chr$(8)
200 get#5, a$
210 if st<>0 then 230
220 get#5, b$
221 a = 0
222 b = 0
224 if a$<>"" then a=asc(a$)
226 if b$<>"" then b=asc(b$)
227 print a;b
228 poke 54272+a, b
230 rem get a$
240 rem if a$<>"" then print#5,a$;
230 goto 200