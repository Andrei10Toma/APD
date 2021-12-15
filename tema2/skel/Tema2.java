import java.io.*;
import java.util.*;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class Tema2 {
    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }
        // Parse the args
        int numberWorkers = Integer.parseInt(args[0]);
        String inputFilePath = args[1];
        String outputFilePath = args[2];
        File inputFile = new File(inputFilePath);
        File outputFile = new File(outputFilePath);

        try (Scanner scanner = new Scanner(inputFile);
             PrintStream printStream = new PrintStream(outputFile)) {
            System.setOut(printStream);
            List<Future<MapResult>> mapResults = new ArrayList<>();
            java.util.Map<String, List<MapResult>> mapForReduce = new HashMap<>();
            int fragmentSize = scanner.nextInt();
            int numberOfFiles = scanner.nextInt();
            ExecutorService executorService = Executors.newFixedThreadPool(numberWorkers);
            startMapTasks(mapResults, scanner, fragmentSize, numberOfFiles, executorService);

            List<ReduceResult> reduceResults = startReduceTasks(mapResults, mapForReduce, executorService);

            // sort the results by the rank of the file
            reduceResults.sort((firstReduceResult, secondReduceResult) -> {
                if (secondReduceResult.getRank() - firstReduceResult.getRank() < 0) {
                    return -1;
                } else if (secondReduceResult.getRank() == firstReduceResult.getRank()) {
                    return 0;
                } else {
                    return 1;
                }
            });

            for (ReduceResult reduceResult : reduceResults) {
                System.out.println(reduceResult);
            }

            executorService.shutdown();
        } catch (FileNotFoundException | InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }
    }

    private static List<ReduceResult> startReduceTasks(List<Future<MapResult>> mapResults, java.util.Map<String, List<MapResult>> mapForReduce, ExecutorService executorService) throws InterruptedException, ExecutionException {
        // the results from the Map are mapped in a new map that will have the relevant information for the next
        // reduce tasks
        for (Future<MapResult> mapResultFuture : mapResults) {
            MapResult mapResult = mapResultFuture.get();
            if (!mapForReduce.containsKey(mapResult.getFileName())) {
                mapForReduce.put(mapResult.getFileName(), new ArrayList<>());
            }
            mapForReduce.get(mapResult.getFileName()).add(mapResult);
        }

        // start the Reduce tasks
        List<Future<ReduceResult>> reduceResultsFuture = new ArrayList<>();
        for (java.util.Map.Entry<String, List<MapResult>> entry : mapForReduce.entrySet()) {
            Future<ReduceResult> reduceResultFuture = executorService.submit(new Reduce(entry.getKey(), entry.getValue()));
            reduceResultsFuture.add(reduceResultFuture);
        }

        // get the results from the Reduce tasks
        List<ReduceResult> reduceResults = new ArrayList<>();
        for (Future<ReduceResult> reduceResultFuture : reduceResultsFuture) {
            reduceResults.add(reduceResultFuture.get());
        }
        return reduceResults;
    }

    private static void startMapTasks(List<Future<MapResult>> mapResults, Scanner scanner, int fragmentSize, int numberOfFiles, ExecutorService executorService) {
        for (int i = 0; i < numberOfFiles; i++) {
            String filePath = scanner.next();
            File file = new File(filePath);
            long fileSize = file.length();
            int currentLength = 0;
            // submit the tasks for the Map
            while (currentLength + fragmentSize < fileSize) {
                Future<MapResult> mapResult = executorService.submit(new Map(filePath, currentLength, fragmentSize));
                currentLength += fragmentSize;
                mapResults.add(mapResult);
            }
            Future<MapResult> lastMapResult = executorService.submit(new Map(filePath, currentLength, (int) (fileSize - currentLength)));
            mapResults.add(lastMapResult);
        }
    }
}
