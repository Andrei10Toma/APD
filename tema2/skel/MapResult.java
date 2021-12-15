import java.util.List;
import java.util.Map;

/**
 * Class for the result returned by the Map task
 */
public class MapResult {
    private Map<Integer, Integer> mapFromLengthToAppearances;
    private String fileName;
    private List<String> maximalWords;

    public MapResult(Map<Integer, Integer> mapFromLengthToAppearances, String fileName, List<String> maximalWords) {
        this.mapFromLengthToAppearances = mapFromLengthToAppearances;
        this.fileName = fileName;
        this.maximalWords = maximalWords;
    }

    public Map<Integer, Integer> getMapFromLengthToAppearances() {
        return mapFromLengthToAppearances;
    }

    public String getFileName() {
        return fileName;
    }

    public List<String> getMaximalWords() {
        return maximalWords;
    }

    public void setMapFromLengthToAppearances(Map<Integer, Integer> mapFromLengthToAppearances) {
        this.mapFromLengthToAppearances = mapFromLengthToAppearances;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public void setMaximalWords(List<String> maximalWords) {
        this.maximalWords = maximalWords;
    }

    @Override
    public String toString() {
        return fileName + "; " + mapFromLengthToAppearances + "; " + maximalWords + "; ";
    }
}
