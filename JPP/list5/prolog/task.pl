% ─── 1. Mergesort ─────────────────────────────────────────────────────────────
mergesort([], []).
mergesort([X], [X]).
mergesort(List, Sorted) :-
    List = [_,_|_],
    split(List, L, R),
    mergesort(L, SL),
    mergesort(R, SR),
    merge_sorted(SL, SR, Sorted).

split([], [], []).
split([X], [X], []).
split([X,Y|Rest], [X|Ls], [Y|Rs]) :-
    split(Rest, Ls, Rs).

merge_sorted([], Ys, Ys).
merge_sorted(Xs, [], Xs).
merge_sorted([X|Xs], [Y|Ys], [X|Rest]) :-
    X =< Y, !,
    merge_sorted(Xs, [Y|Ys], Rest).
merge_sorted([X|Xs], [Y|Ys], [Y|Rest]) :-
    merge_sorted([X|Xs], Ys, Rest).

% ─── 2. Diophantine equation ──────────────────────────────────────────────────
de(A, 0, 1, 0, A) :- !.
de(A, B, X, Y, G) :-
    R is A mod B,
    de(B, R, X1, Y1, G),
    X is Y1,
    Y is X1 - (A // B) * Y1.

% ─── 3. Prime factors ─────────────────────────────────────────────────────────
prime_factors(N, Factors) :-
    prime_factors(N, 2, Factors).

prime_factors(1, _, []) :- !.
prime_factors(N, F, [N]) :-
    F * F > N, !.
prime_factors(N, F, [F|Rest]) :-
    N mod F =:= 0, !,
    N1 is N // F,
    prime_factors(N1, F, Rest).
prime_factors(N, F, Factors) :-
    F1 is F + 1,
    prime_factors(N, F1, Factors).

% ─── 4. Totient ───────────────────────────────────────────────────────────────
gcd(A, 0, A) :- !.
gcd(A, B, G) :-
    R is A mod B,
    gcd(B, R, G).

totient(N, T) :-
    totient(N, N, 0, T).

totient(_, 0, Acc, Acc) :- !.
totient(N, K, Acc, T) :-
    gcd(K, N, G),
    ( G =:= 1 -> Acc1 is Acc + 1 ; Acc1 is Acc ),
    K1 is K - 1,
    totient(N, K1, Acc1, T).

% ─── 5. Primes up to N (sieve) ────────────────────────────────────────────────
primes(N, Primes) :-
    numlist(2, N, List),
    sieve(List, Primes).

sieve([], []).
sieve([P|Rest], [P|Primes]) :-
    exclude(divisible_by(P), Rest, Filtered),
    sieve(Filtered, Primes).

divisible_by(P, X) :- X mod P =:= 0.