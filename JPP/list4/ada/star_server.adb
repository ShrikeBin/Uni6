with Ada.Text_IO;         use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Ada.Numerics.Discrete_Random;

procedure Star_Server is

   Num_Users     : constant := 10;
   Msgs_Per_User : constant := 10;

   package Rand_Int is new Ada.Numerics.Discrete_Random (Integer);
   Gen : Rand_Int.Generator;

   protected Logger is
      procedure Log (Who : String; Msg : String);
   end Logger;
   protected body Logger is
      procedure Log (Who : String; Msg : String) is
      begin
         Put_Line ("[" & Who & "] " & Msg);
      end Log;
   end Logger;

   function UName (Id : Integer) return String is
      S : constant String := Integer'Image (Id);
   begin
      return "User " & S (S'First + 1 .. S'Last);
   end UName;

   type Count_Array is array (0 .. Num_Users - 1) of Integer;
   protected Recv_Counter is
      procedure Increment (Id : Integer);
      function Get (Id : Integer) return Integer;
   private
      Counts : Count_Array := (others => 0);
   end Recv_Counter;
   protected body Recv_Counter is
      procedure Increment (Id : Integer) is
      begin Counts (Id) := Counts (Id) + 1; end Increment;
      function Get (Id : Integer) return Integer is
      begin return Counts (Id); end Get;
   end Recv_Counter;

   -- Barrier: main blocks until all users finish
   protected Barrier is
      procedure Signal;
      entry Wait_All;
   private
      Done_Count : Integer := 0;
   end Barrier;
   protected body Barrier is
      procedure Signal is
      begin Done_Count := Done_Count + 1; end Signal;
      entry Wait_All when Done_Count = Num_Users is
      begin null; end Wait_All;
   end Barrier;

   type Message is record
      From    : Integer;
      To_User : Integer;
      Content : Integer;
   end record;

   -- User receiver task: only accepts deliveries and Done signal
   task type Receiver_Task is
      entry Start   (Id : Integer);
      entry Deliver (M  : Message);
      entry Done;
   end Receiver_Task;

   -- Server task
   task Server is
      entry Send (M : Message);
   end Server;

   -- Sender task: one per user, sends Msgs_Per_User messages then exits
   task type Sender_Task is
      entry Start (Id : Integer);
   end Sender_Task;

   Receivers : array (0 .. Num_Users - 1) of Receiver_Task;

   task body Server is
      Total   : constant Integer := Num_Users * Msgs_Per_User;
      Relayed : Integer := 0;
      Msg     : Message;
   begin
      Logger.Log ("SERVER", "started, expecting" & Integer'Image (Total) & " messages");
      while Relayed < Total loop
         accept Send (M : Message) do Msg := M; end Send;
         Logger.Log ("SERVER",
                     "relaying from " & UName (Msg.From) &
                     " to "           & UName (Msg.To_User) &
                     " (seq="         & Integer'Image (Msg.Content) & ")");
         Receivers (Msg.To_User).Deliver (Msg);
         Recv_Counter.Increment (Msg.To_User);
         Relayed := Relayed + 1;
      end loop;
      Logger.Log ("SERVER", "all messages delivered, signalling receivers.");
      for I in 0 .. Num_Users - 1 loop
         Receivers (I).Done;
      end loop;
      Logger.Log ("SERVER", "shutdown complete.");
   end Server;

   task body Receiver_Task is
      My_Id   : Integer;
      Running : Boolean := True;
      Msg     : Message;
   begin
      accept Start (Id : Integer) do My_Id := Id; end Start;
      Logger.Log (UName (My_Id), "started");
      while Running loop
         select
            accept Deliver (M : Message) do Msg := M; end Deliver;
            Logger.Log (UName (My_Id),
                        "received from " & UName (Msg.From) &
                        " (seq=" & Integer'Image (Msg.Content) & ")");
         or
            accept Done;
            Running := False;
         end select;
      end loop;
      Logger.Log (UName (My_Id), "exiting.");
      Barrier.Signal;
   end Receiver_Task;

   task body Sender_Task is
      My_Id : Integer;
      Dest  : Integer;
   begin
      accept Start (Id : Integer) do My_Id := Id; end Start;
      for Seq in 1 .. Msgs_Per_User loop
         delay Duration (Float (Rand_Int.Random (Gen) mod 4 + 1) * 0.03);
         Dest := Rand_Int.Random (Gen) mod Num_Users;
         Logger.Log (UName (My_Id),
                     "sending msg #" & Integer'Image (Seq) &
                     " to " & UName (Dest));
         Server.Send ((From => My_Id, To_User => Dest, Content => Seq));
      end loop;
      Logger.Log (UName (My_Id), "finished sending.");
   end Sender_Task;

   -- Senders are declared after Receivers so they can reference Server
   Senders : array (0 .. Num_Users - 1) of Sender_Task;

begin
   Rand_Int.Reset (Gen);
   Put_Line ("=== Star Server (Ada) ===");
   Put_Line ("Users         :" & Integer'Image (Num_Users));
   Put_Line ("Messages each :" & Integer'Image (Msgs_Per_User));
   Put_Line ("========================");

   for I in 0 .. Num_Users - 1 loop
      Receivers (I).Start (I);
      Senders (I).Start (I);
   end loop;

   Barrier.Wait_All;

   Put_Line ("");
   Put_Line ("=== FINAL REPORT ===");
   for I in 0 .. Num_Users - 1 loop
      Put (UName (I) & " received: ");
      Put (Recv_Counter.Get (I), Width => 1);
      Put_Line (" message(s)");
   end loop;
end Star_Server;