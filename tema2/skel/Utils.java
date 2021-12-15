import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class Utils {
    private Utils() {}

    // Regex used for the split of the partition of the document
    protected static final String WORD_DELIMITER = "[;:/?~\\\\.,><`\\[\\]{}()!@#$%^&\\-_+'=*\" \t\n\r]";

    // Word delimiter set
    protected static final Set<Character> WORD_DELIMITER_SET = new HashSet<>(Arrays.asList(';', ':', '/', '?', '~', '\\',
            '.', ',', '>', '<', '`', '[', ']', '{', '}', '(', ')', '!', '@', '#', '$', '%', '^', '&', '-', '_', '+',
            '\'', '=', '*', '"', ' ','\t', '\n', '\r'));

    // Computes the n-th fibonacci number
    static int fibo(int n) {
        int first = 0;
        int second = 1;
        int third;

        if (n == 0)
            return first;

        for (int i = 2; i <= n; i++) {
            third = first + second;
            first = second;
            second = third;
        }

        return second;
    }
}
