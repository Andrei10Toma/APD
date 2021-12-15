import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

/**
 * Class that will implement the Reduce tasks
 */
public class Reduce implements Callable<ReduceResult> {
    private final String documentName;
    private final List<MapResult> mapResults;

    public Reduce(String documentName, List<MapResult> mapResults) {
        this.documentName = documentName;
        this.mapResults = mapResults;
    }

    @Override
    public ReduceResult call() throws Exception {
        NumberOfWordsAndMaximalLength numberOfWordsAndMaximalLength = getNumberOfWordsAndMaximalLength();
        int numberOfWords = numberOfWordsAndMaximalLength.numberOfWords;
        int maximalLength = numberOfWordsAndMaximalLength.maximalLength;
        int numberOfWordsOfMaximalLength = getNumberOfWordsOfMaximalLength(maximalLength);
        double rank = computeRank(maximalLength, numberOfWords);

        return new ReduceResult(documentName, maximalLength, numberOfWordsOfMaximalLength, rank);
    }

    private int getNumberOfWordsOfMaximalLength(int maximalLength) {
        int numberOfWordsOfMaximalLength = 0;
        for (MapResult mapResult : mapResults) {
            numberOfWordsOfMaximalLength += mapResult.getMapFromLengthToAppearances().getOrDefault(maximalLength, 0);
        }
        return numberOfWordsOfMaximalLength;
    }

    private double computeRank(int maximalLength, int numberOfWords) {
        if (numberOfWords == 0) {
            return 0;
        }

        double rank = 0;
        for (int i = 1; i <= maximalLength; i++) {
            rank += (Utils.fibo(i + 1) * (double)getNumberOfWordsOfLength(i)) / (double)numberOfWords;
        }
        return rank;
    }

    private NumberOfWordsAndMaximalLength getNumberOfWordsAndMaximalLength() {
        int numberOfWords = 0;
        int maximalLength = 0;
        for (MapResult mapResult : mapResults) {
            for (Map.Entry<Integer, Integer> entry : mapResult.getMapFromLengthToAppearances().entrySet()) {
                maximalLength = Math.max(entry.getKey(), maximalLength);
                numberOfWords += entry.getValue();
            }
        }
        return new NumberOfWordsAndMaximalLength(numberOfWords, maximalLength);
    }

    private int getNumberOfWordsOfLength(int length) {
        int numberOfWordsOfLength = 0;
        for (MapResult mapResult : mapResults) {
            numberOfWordsOfLength += mapResult.getMapFromLengthToAppearances().getOrDefault(length, 0);
        }

        return numberOfWordsOfLength;
    }

    private static class NumberOfWordsAndMaximalLength {
        private final int numberOfWords;
        private final int maximalLength;

        public NumberOfWordsAndMaximalLength(int numberOfWords, int maximalLength) {
            this.numberOfWords = numberOfWords;
            this.maximalLength = maximalLength;
        }
    }
}
