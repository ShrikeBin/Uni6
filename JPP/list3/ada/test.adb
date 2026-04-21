-- test.adb  —  RSA + Diffie-Hellman tests in Ada
with Ada.Text_IO;         use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with RSA_Pkg;
with DH_Pkg;

procedure Test is

   -- ── RSA: ring mod 10007 * 10009 ──────────────────────────────────────────
   package RSA is new RSA_Pkg (P => 10007, Q => 10009);

   -- ── DH: field of characteristic 1234567891 ────────────────────────────────
   package DH is new DH_Pkg (P => 1234567891);

   procedure Check (Got, Expected : Natural; Label : String) is
   begin
      Put ("  " & Label & " = ");
      Put (Got, Width => 0);
      if Got = Expected then Put_Line ("  ✓");
      else
         Put ("  ✗  (expected ");
         Put (Expected, Width => 0);
         Put_Line (")");
      end if;
   end Check;

begin
   -- ──────────────────────────────────────────────────────────────────────────
   Put_Line ("══════════════════════════════════════");
   Put_Line ("  RSA test  (ring mod 10007 * 10009)");
   Put_Line ("══════════════════════════════════════");

   RSA.Init;
   Put ("Modulus n   = "); Put (RSA.Get_Modulo,     Width => 0); New_Line;
   Put ("Public key  = "); Put (RSA.Get_Public_Key,  Width => 0); New_Line;
   New_Line;

   declare
      Messages : constant array (1 .. 5) of Natural :=
        (1, 42, 1234, 99999, 100160062);
   begin
      for M of Messages loop
         declare
            C : constant Natural := RSA.Encrypt (M);
            D : constant Natural := RSA.Decrypt (C);
         begin
            Put ("  m="); Put (M, Width => 0);
            Put ("  c="); Put (C, Width => 0);
            Put ("  d="); Put (D, Width => 0);
            if D = M then Put_Line ("  ✓");
            else          Put_Line ("  ✗ MISMATCH");
            end if;
         end;
      end loop;
   end;

   New_Line;

   -- ──────────────────────────────────────────────────────────────────────────
   Put_Line ("═══════════════════════════════════════════════");
   Put_Line ("  Diffie-Hellman test  (field mod 1234567891)");
   Put_Line ("═══════════════════════════════════════════════");

   DH.DH_Init;
   Put ("Generator g = "); Put (DH.Get_Generator, Width => 0); New_Line;
   New_Line;

   declare
      Alice : DH.User := DH.Make_User;
      Bob   : DH.User := DH.Make_User;
      A_Pub : constant DH.Element := DH.Get_Public_Key (Alice);
      B_Pub : constant DH.Element := DH.Get_Public_Key (Bob);

      Messages : constant array (1 .. 4) of Natural :=
        (1, 7, 999, 123456);
   begin
      Put ("Alice public key = "); Put (A_Pub, Width => 0); New_Line;
      Put ("Bob   public key = "); Put (B_Pub, Width => 0); New_Line;
      New_Line;

      DH.Set_Key (Alice, B_Pub);
      DH.Set_Key (Bob,   A_Pub);

      Put_Line ("Alice encrypts, Bob decrypts:");
      for M of Messages loop
         declare
            C : constant DH.Element := DH.Encrypt (Alice, M);
            D : constant DH.Element := DH.Decrypt (Bob,   C);
         begin
            Put ("  m="); Put (M, Width => 0);
            Put ("  c="); Put (C, Width => 0);
            Put ("  d="); Put (D, Width => 0);
            if D = M then Put_Line ("  ✓");
            else          Put_Line ("  ✗ MISMATCH");
            end if;
         end;
      end loop;

      New_Line;
      Put_Line ("Bob encrypts, Alice decrypts:");
      for M of Messages loop
         declare
            C : constant DH.Element := DH.Encrypt (Bob,   M);
            D : constant DH.Element := DH.Decrypt (Alice, C);
         begin
            Put ("  m="); Put (M, Width => 0);
            Put ("  c="); Put (C, Width => 0);
            Put ("  d="); Put (D, Width => 0);
            if D = M then Put_Line ("  ✓");
            else          Put_Line ("  ✗ MISMATCH");
            end if;
         end;
      end loop;

      New_Line;
      Put_Line ("Error-handling test (encrypt without setKey):");
      declare
         Charlie : DH.User := DH.Make_User;
         Dummy   : DH.Element;
      begin
         Dummy := DH.Encrypt (Charlie, 42);
         Put_Line ("  ✗ Exception NOT raised!");
         pragma Unreferenced (Dummy);
      exception
         when DH.Key_Not_Set =>
            Put_Line ("  ✓ Caught expected Key_Not_Set exception");
      end;
   end;

end Test;