pragma Warnings (Off);
pragma Ada_95;
pragma Source_File_Name (ada_main, Spec_File_Name => "b__led_blink.ads");
pragma Source_File_Name (ada_main, Body_File_Name => "b__led_blink.adb");
pragma Suppress (Overflow_Check);

package body ada_main is

   E05 : Short_Integer; pragma Import (Ada, E05, "ada__text_io_E");
   E15 : Short_Integer; pragma Import (Ada, E15, "riscv__csr_generic_E");
   E10 : Short_Integer; pragma Import (Ada, E10, "interrupts_E");
   E27 : Short_Integer; pragma Import (Ada, E27, "runtime_support_E");
   E29 : Short_Integer; pragma Import (Ada, E29, "uart0_E");


   procedure adainit is
   begin
      null;

      Ada.Text_Io'Elab_Body;
      E05 := E05 + 1;
      E15 := E15 + 1;
      Interrupts'Elab_Body;
      E10 := E10 + 1;
      E27 := E27 + 1;
      E29 := E29 + 1;
   end adainit;

   procedure Ada_Main_Program;
   pragma Import (Ada, Ada_Main_Program, "_ada_led_blink");

   procedure main is
      Ensure_Reference : aliased System.Address := Ada_Main_Program_Name'Address;
      pragma Volatile (Ensure_Reference);

   begin
      adainit;
      Ada_Main_Program;
   end;

--  BEGIN Object file/option list
   --   /home/dipen/Downloads/neorv32-halv2/led_blink/obj/development/runtime_support.o
   --   /home/dipen/Downloads/neorv32-halv2/led_blink/obj/development/led_blink.o
   --   -L/home/dipen/Downloads/neorv32-halv2/led_blink/obj/development/
   --   -L/home/dipen/Downloads/neorv32-halv2/led_blink/obj/development/
   --   -L/home/dipen/.local/share/alire/builds/bare_runtime_14.0.0_095db6f0/282b01b920f0d5bb2bac604ac6d9e811f26d175144bc99af963e0381e797ee94/adalib/
   --   -L/home/dipen/Downloads/neorv32-halv2/lib/
   --   -static
   --   -lgnat
--  END Object file/option list   

end ada_main;
