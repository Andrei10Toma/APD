/**
 * Class for the result returned by the Reduce task
 */
public class ReduceResult {
    private final String documentName;
    private final int maxLength;
    private final int numberOfWordsMaxLength;
    private final double rank;

    public ReduceResult(String documentName, int maxLength, int numberOfWordsMaxLength, double rank) {
        this.documentName = documentName;
        this.maxLength = maxLength;
        this.numberOfWordsMaxLength = numberOfWordsMaxLength;
        this.rank = rank;
    }

    public String getDocumentName() {
        return documentName;
    }

    public int getMaxLength() {
        return maxLength;
    }

    public int getNumberOfWordsMaxLength() {
        return numberOfWordsMaxLength;
    }

    public double getRank() {
        return rank;
    }

    @Override
    public String toString() {
        return documentName + "," + String.format("%.2f", rank) + "," + maxLength + "," + numberOfWordsMaxLength;
    }
}
