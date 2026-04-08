with Ada.Text_IO; use Ada.Text_IO;
with Ring;
with Ada.Exceptions; use Ada.Exceptions;

procedure Test is

   generic
      N : Positive;
   procedure Run_Test (A, B : Integer);

   procedure Run_Test (A, B : Integer) is
      package Ring_N is new Ring (N => N);
      use Ring_N;

      X : Element := To_Element (A);
      Y : Element := To_Element (B);
   begin
      Put_Line ("Testing with base" & N'Image & ", a =" & A'Image & ", b =" & B'Image & ":");
      
      Put ("x:      "); Put_Line (X);
      Put ("y:      "); Put_Line (Y);
      Put ("x + y:  "); Put_Line (X + Y);
      Put ("x - y:  "); Put_Line (X - Y);
      Put ("-x:     "); Put_Line (-X);
      Put ("x * y:  "); Put_Line (X * Y);

      begin
         Put ("x / y:  ");
         Put_Line (X / Y);
      exception
         when E : Division_By_Zero | No_Inverse =>
            Put_Line ("Error during division: " & Exception_Message (E));
      end;

      New_Line;
   end Run_Test;

   procedure Test_Mod_10 is new Run_Test (N => 10);
   procedure Test_Mod_5  is new Run_Test (N => 5);
   procedure Test_Mod_71 is new Run_Test (N => 71);
   procedure Test_Mod_23 is new Run_Test (N => 23);
   procedure Test_Mod_12 is new Run_Test (N => 12);
   procedure Test_Mod_8  is new Run_Test (N => 8);

begin
   Test_Mod_10 (3, 7);
   Test_Mod_5  (2, 4);
   Test_Mod_71 (36, 45);
   Test_Mod_23 (13, 4);
   Test_Mod_12 (5, 0);
   Test_Mod_8  (5, 4);
end Test;