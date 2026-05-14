let () =
  (* helpers *)
  let print_int_list lst =
    print_string "[";
    List.iteri (fun i x ->
      if i > 0 then print_string "; ";
      print_int x) lst;
    print_string "]\n"
  in

  print_endline "=== binomial ===";
  Printf.printf "%d\n" (Task.binomial 10 3);   (* 120 *)
  Printf.printf "%d\n" (Task.binomial 6  3);   (* 20  *)
  Printf.printf "%d\n" (Task.binomial 0  0);   (* 1   *)

  print_endline "\n=== binomial2 (Pascal) ===";
  Printf.printf "%d\n" (Task.binomial2 10 3);  (* 120 *)
  Printf.printf "%d\n" (Task.binomial2 6  3);  (* 20  *)

  print_endline "\n=== mergesort ===";
  print_int_list (Task.mergesort [5; 3; 8; 1; 9; 2; 7; 4; 6]);
  print_int_list (Task.mergesort []);
  print_int_list (Task.mergesort [1]);

  print_endline "\n=== de (Diophantine) ===";
  let show_de a b =
    let (x, y, z) = Task.de a b in
    Printf.printf "de %d %d = (%d, %d, %d)\n" a b x y z
  in
  show_de 30 20;
  show_de 35 15;
  show_de 7  3;

  print_endline "\n=== prime_factors ===";
  print_int_list (Task.prime_factors 360);
  print_int_list (Task.prime_factors 13);
  print_int_list (Task.prime_factors 1);

  print_endline "\n=== totient ===";
  Printf.printf "%d\n" (Task.totient 36);   (* 12 *)
  Printf.printf "%d\n" (Task.totient 1);    (* 1  *)
  Printf.printf "%d\n" (Task.totient 7);    (* 6  *)

  print_endline "\n=== totient2 ===";
  Printf.printf "%d\n" (Task.totient2 36);  (* 12 *)
  Printf.printf "%d\n" (Task.totient2 1);   (* 1  *)
  Printf.printf "%d\n" (Task.totient2 7);   (* 6  *)

  print_endline "\n=== primes ===";
  print_int_list (Task.primes 50);
  print_int_list (Task.primes 2);
  print_int_list (Task.primes 1)