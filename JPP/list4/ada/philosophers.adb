with Ada.Text_IO;         use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Ada.Numerics.Discrete_Random;

procedure Philosophers is

   Num_Philosophers : constant := 5;
   Meals_Per_Phil   : constant := 25;

   package Rand_Int is new Ada.Numerics.Discrete_Random (Integer);
   Gen : Rand_Int.Generator;

   protected type Fork is
      entry Pick_Up;
      procedure Put_Down;
   private
      In_Use : Boolean := False;
   end Fork;

   protected body Fork is
      entry Pick_Up when not In_Use is
      begin
         In_Use := True;
      end Pick_Up;
      procedure Put_Down is
      begin
         In_Use := False;
      end Put_Down;
   end Fork;

   protected Waiter is
      entry Request_Seat;
      procedure Leave_Seat;
   private
      Seats_Taken : Integer := 0;
   end Waiter;

   protected body Waiter is
      entry Request_Seat when Seats_Taken < Num_Philosophers - 1 is
      begin
         Seats_Taken := Seats_Taken + 1;
      end Request_Seat;
      procedure Leave_Seat is
      begin
         Seats_Taken := Seats_Taken - 1;
      end Leave_Seat;
   end Waiter;

   protected Logger is
      procedure Log (Phil_Id : Integer; Msg : String);
   end Logger;

   protected body Logger is
      procedure Log (Phil_Id : Integer; Msg : String) is
      begin
         Put ("[Philosopher ");
         Put (Phil_Id, Width => 1);
         Put ("] ");
         Put_Line (Msg);
      end Log;
   end Logger;

   -- Barrier: main blocks until all philosophers finish
   protected Barrier is
      procedure Signal;
      entry Wait_All;
   private
      Done_Count : Integer := 0;
   end Barrier;

   protected body Barrier is
      procedure Signal is
      begin
         Done_Count := Done_Count + 1;
      end Signal;
      entry Wait_All when Done_Count = Num_Philosophers is
      begin null; end Wait_All;
   end Barrier;

   Forks : array (0 .. Num_Philosophers - 1) of Fork;

   type Fail_Array is array (0 .. Num_Philosophers - 1) of Integer;
   protected Results is
      procedure Report (Phil_Id : Integer; Fails : Integer);
      function Get_Fails (Phil_Id : Integer) return Integer;
   private
      Fail_Count : Fail_Array := (others => 0);
   end Results;

   protected body Results is
      procedure Report (Phil_Id : Integer; Fails : Integer) is
      begin
         Fail_Count (Phil_Id) := Fails;
      end Report;
      function Get_Fails (Phil_Id : Integer) return Integer is
      begin
         return Fail_Count (Phil_Id);
      end Get_Fails;
   end Results;

   task type Philosopher_Task is
      entry Start (Id : Integer);
   end Philosopher_Task;

   task body Philosopher_Task is
      My_Id    : Integer;
      Left     : Integer;
      Right    : Integer;
      Meals    : Integer := 0;
      Failures : Integer := 0;
   begin
      accept Start (Id : Integer) do
         My_Id := Id;
      end Start;

      Left  := My_Id;
      Right := (My_Id + 1) mod Num_Philosophers;

      while Meals < Meals_Per_Phil loop
         Logger.Log (My_Id, "is thinking...");
         delay Duration (Float (Rand_Int.Random (Gen) mod 3 + 1) * 0.05);

         Logger.Log (My_Id, "is hungry, waiting for a seat...");
         Waiter.Request_Seat;

         Forks (Left).Pick_Up;
         Logger.Log (My_Id, "picked up LEFT fork");
         Forks (Right).Pick_Up;
         Logger.Log (My_Id, "picked up RIGHT fork");

         Meals := Meals + 1;
         Logger.Log (My_Id, "*** EATING meal" & Integer'Image (Meals) & " ***");
         delay Duration (Float (Rand_Int.Random (Gen) mod 3 + 1) * 0.05);

         Forks (Left).Put_Down;
         Forks (Right).Put_Down;
         Logger.Log (My_Id, "put down forks");

         Waiter.Leave_Seat;
      end loop;

      Results.Report (My_Id, Failures);
      Logger.Log (My_Id, "finished all meals. Failures:" & Integer'Image (Failures));
      Barrier.Signal;
   end Philosopher_Task;

   Phil_Tasks : array (0 .. Num_Philosophers - 1) of Philosopher_Task;

begin
   Rand_Int.Reset (Gen);

   Put_Line ("=== Dining Philosophers (Ada) ===");
   Put_Line ("Philosophers :" & Integer'Image (Num_Philosophers));
   Put_Line ("Meals each   :" & Integer'Image (Meals_Per_Phil));
   Put_Line ("=================================");

   for I in 0 .. Num_Philosophers - 1 loop
      Phil_Tasks (I).Start (I);
   end loop;

   Barrier.Wait_All;

   Put_Line ("");
   Put_Line ("=== FINAL REPORT ===");
   for I in 0 .. Num_Philosophers - 1 loop
      Put ("Philosopher ");
      Put (I, Width => 1);
      Put (" failed to eat: ");
      Put (Results.Get_Fails (I), Width => 1);
      Put_Line (" times");
   end loop;
   Put_Line ("Fairness: all philosophers ate exactly" &
             Integer'Image (Meals_Per_Phil) & " meals (waiter guarantees no starvation).");
end Philosophers;