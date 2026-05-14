:- consult(task).

run_tests :-
    write('=== mergesort ==='), nl,
    mergesort([5,3,8,1,9,2,7,4,6], S1), write(S1), nl,
    mergesort([], S2),                   write(S2), nl,
    mergesort([1], S3),                  write(S3), nl,

    nl, write('=== de (Diophantine) ==='), nl,
    de(30, 20, X1, Y1, Z1), format("de(30,20) = (~w, ~w, ~w)~n", [X1,Y1,Z1]),
    de(35, 15, X2, Y2, Z2), format("de(35,15) = (~w, ~w, ~w)~n", [X2,Y2,Z2]),
    de(7,   3, X3, Y3, Z3), format("de(7,3)   = (~w, ~w, ~w)~n", [X3,Y3,Z3]),

    nl, write('=== prime_factors ==='), nl,
    prime_factors(360, F1), write(F1), nl,
    prime_factors(13,  F2), write(F2), nl,
    prime_factors(1,   F3), write(F3), nl,

    nl, write('=== totient ==='), nl,
    totient(36, T1), write(T1), nl,
    totient(1,  T2), write(T2), nl,
    totient(7,  T3), write(T3), nl,

    nl, write('=== primes ==='), nl,
    primes(50, P1), write(P1), nl,
    primes(2,  P2), write(P2), nl,
    write([]), nl.

:- run_tests.