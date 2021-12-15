import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.Callable;

/**
 * Class that will implement the Map tasks
 */
public class Map implements Callable<MapResult> {
    private final String documentName;
    private final int startOffset;
    private final int fragmentLength;

    public Map(String documentName, int startOffset, int fragmentLength) {
        this.documentName = documentName;
        this.startOffset = startOffset;
        this.fragmentLength = fragmentLength;
    }

    @Override
    public MapResult call() {
        try (RandomAccessFile randomAccessFile = new RandomAccessFile(documentName, "r")) {
            // the pointer of the file will be at the beginning of the document partition
            randomAccessFile.seek(startOffset);
            File file = new File(documentName);
            int currentOffset = startOffset;
            int endOffset = startOffset + fragmentLength;

            if (startOffsetIsInMiddleOfWord(randomAccessFile)) {
                // move the cursor so that the word will not be considered as part of the partition
                currentOffset = changeTheStartOffset(randomAccessFile);
            }

            if (endOffsetIsInMiddleOfWord(randomAccessFile)) {
                // move the cursor so that the word will be considered as part of the partition
                endOffset = changeTheEndOffset(randomAccessFile);
            }

            String[] splitPartitionStrings = getPartitionedString(randomAccessFile, currentOffset, endOffset);
            MapResult result = new MapResult(new HashMap<>(), file.getName(), new ArrayList<>());
            int maxLength = 0;
            if (splitPartitionStrings.length != 0) {
                maxLength = computeMaxLengthAndMapOfPartition(splitPartitionStrings, result);
                computeMaximalWords(splitPartitionStrings, result, maxLength);
            }
            return result;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    private void computeMaximalWords(String[] splitPartitionStrings, MapResult result, int maxLength) {
        for (String splitPartitionString : splitPartitionStrings) {
            if (splitPartitionString.length() == maxLength) {
                result.getMaximalWords().add(splitPartitionString);
            }
        }
    }

    private int computeMaxLengthAndMapOfPartition(String[] splitPartitionStrings, MapResult result) {
        int maxLength;
        maxLength = splitPartitionStrings[0].length();
        for (String splitPartitionString : splitPartitionStrings) {
            if (splitPartitionString.length() != 0) {
                int splitPartitionLength = splitPartitionString.length();
                maxLength = Math.max(maxLength, splitPartitionLength);
                if (result.getMapFromLengthToAppearances().containsKey(splitPartitionLength)) {
                    result.getMapFromLengthToAppearances().put(splitPartitionLength,
                            result.getMapFromLengthToAppearances().get(splitPartitionLength) + 1);
                } else {
                    result.getMapFromLengthToAppearances().put(splitPartitionLength, 1);
                }
            }
        }
        return maxLength;
    }

    private String[] getPartitionedString(RandomAccessFile randomAccessFile, int currentOffset, int endOffset) throws IOException {
        byte[] readBytes = new byte[endOffset - currentOffset];
        randomAccessFile.seek(currentOffset);
        randomAccessFile.readFully(readBytes);
        StringBuilder partition = new StringBuilder();
        for (byte readByte : readBytes) {
            partition.append((char) readByte);
        }
        String partitionString = partition.toString();
        return partitionString.split(Utils.WORD_DELIMITER);
    }

    private int changeTheEndOffset(RandomAccessFile randomAccessFile) throws IOException {
        int endOffset = startOffset + fragmentLength - 1;
        return computeNewOffset(randomAccessFile, endOffset);
    }

    private boolean endOffsetIsInMiddleOfWord(RandomAccessFile randomAccessFile) throws IOException {
        int endOffset = startOffset + fragmentLength - 1;
        randomAccessFile.seek(endOffset);
        byte[] character = new byte[1];
        int returnValue = randomAccessFile.read(character);
        // if the read character is not a word delimiter
        if (returnValue != -1 && !Utils.WORD_DELIMITER_SET.contains((char) character[0])) {
            // read the character from the next position
            randomAccessFile.seek((long) endOffset + 1);
            returnValue = randomAccessFile.read(character);
            // reached end of file
            if (returnValue == -1) {
                return false;
            }
            // if the character from the next position is not a word delimiter then the end offset is in the middle of a word
            return !Utils.WORD_DELIMITER_SET.contains((char) character[0]);
        }

        return false;
    }

    private int changeTheStartOffset(RandomAccessFile randomAccessFile) throws IOException {
        return computeNewOffset(randomAccessFile, startOffset);
    }

    private boolean startOffsetIsInMiddleOfWord(RandomAccessFile randomAccessFile) throws IOException {
        randomAccessFile.seek(startOffset);
        byte[] character = new byte[1];
        int returnValue = randomAccessFile.read(character);
        // if it is not the first partition and didn't reach the end of the word the read character is not a word delimiter
        if (startOffset != 0 && returnValue != -1 && !Utils.WORD_DELIMITER_SET.contains((char) character[0])) {
            // read the character from the previous position
            randomAccessFile.seek((long) startOffset - 1);
            returnValue = randomAccessFile.read(character);
            // reached end of file
            if (returnValue == -1) {
                return false;
            }
            // if the previous character is not a word delimiter then we are in the middle of a word
            return !Utils.WORD_DELIMITER_SET.contains((char) character[0]);
        }

        return false;
    }

    private int computeNewOffset(RandomAccessFile randomAccessFile, int currentOffset) throws IOException {
        randomAccessFile.seek(currentOffset);
        byte[] character = new byte[1];
        int returnValue = randomAccessFile.read(character);
        // seek the offset of the file with a position forward until the first word delimiter is met
        // doing this the word will be ignored if the start offset is in the middle of a word and considered in the
        // partition if the end offset is in the middle of a word
        while (!Utils.WORD_DELIMITER_SET.contains((char) character[0]) && returnValue != -1) {
            returnValue = randomAccessFile.read(character);
            currentOffset++;
        }
        return currentOffset;
    }
}
