import java.io.*;
import java.util.*;
import java.nio.file.*;

public class TestCompiler {
    public static void main(String args[]) {
        if (args.length < 2) {
            System.err.println("Usage: TestCompiler <code file> <data file>");
            return;
        }
		
		if (args.length > 2) {
			// parse optimization parameters
		}
		
        try {
            // Redirect System.in from DLX to data file
            InputStream origIn = System.in,
                        newIn = new BufferedInputStream(
                                new FileInputStream(args[1]));
            System.setIn(newIn);

            // Compiler p = new Compiler(args[0]);
            // int prog[] = p.getProgram();

            int[] prog = Files.lines(Paths.get(args[0]))
                  .mapToInt(Integer::parseInt).toArray();

            // int prog[] = {1130364929, -870645760, -1006632960};
            if (prog == null) {
                System.err.println("Error compiling program!");
                return;
            }

            DLX.load(prog);
            DLX.execute();

            System.setIn(origIn);
            newIn.close();
        } catch (IOException e) {
            System.err.println("Error reading input files!");
        }
    }

}
