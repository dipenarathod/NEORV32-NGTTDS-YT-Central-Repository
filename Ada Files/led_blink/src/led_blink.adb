with System;
with Runtime_Support;
with Interrupts;
with neorv32.UART0; use neorv32.UART0;
with neorv32;
with Uart0;
with Interfaces;    use Interfaces;
with Ada.Text_IO;   use Ada.Text_IO;
with neorv32;       use neorv32;
with System;

with Ada.Text_IO;     use Ada.Text_IO;
with neorv32.SYSINFO; use neorv32.SYSINFO;
with NEORV32.GPIO;    use NEORV32.GPIO;

procedure Led_Blink is
   Dummy : UInt32 := 0;
   pragma Volatile (Dummy);  -- prevents optimizer from eliminating the loop
begin
   loop
      --  GPIO_Periph.INPUT0 := 0;
      GPIO_Periph.INPUT1 := 0;
      --  GPIO_Periph.OUTPUT0 := 0;
      --  GPIO_Periph.OUTPUT1 := 0;
      for I in 1 .. 20_000_00 loop
         Dummy := Dummy + 1;
      end loop;

      --  GPIO_Periph.INPUT0 := 3;
      GPIO_Periph.INPUT1 := 3;
      --  GPIO_Periph.OUTPUT0 := 1;
      --  GPIO_Periph.OUTPUT1 := 1;
      for I in 1 .. 20_000_00 loop
         Dummy := Dummy + 1;
      end loop;
   end loop;
end Led_Blink;
