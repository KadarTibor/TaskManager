
import javafx.application.Application;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.*;
import java.net.Socket;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

public class ClientApplication extends Application {

    public static ObservableList<String> processData = FXCollections.observableArrayList();
    public static int time = -1;
    public static double cpuUsage;

    @Override
    public void start(Stage primaryStage) throws Exception {

        Thread listenerThread = new Thread( new Runnable(){

            public void run() {

                try {
                    Socket serverSocket = new Socket("localhost", 27015);
                    //OutputStream outputStream = serverSocket.getOutputStream();
                    InputStream inputStream = serverSocket.getInputStream();

                    //read from the sever
                    while(true){
                        String str = readString(inputStream);
                        int nrOfMessages = 0;
                        try{
                            nrOfMessages = Integer.parseInt(str);
                        } catch (Exception e){
                            continue;
                        }
                        List<String> processes = new ArrayList<>();
                        for(int i= 0; i < nrOfMessages - 1; i++){
                            String msg = readString(inputStream);
                            processes.add(msg);
                        }

                        String cu = readString(inputStream);
                        double totalCpuUsage;
                        try{
                            totalCpuUsage = Double.parseDouble(cu);
                        } catch (Exception e){
                            totalCpuUsage = cpuUsage;
                        }

                        double cpupercentage = totalCpuUsage;

                        Platform.runLater(()-> {
                            processData.clear();
                            processData.addAll(processes);
                            time++;
                            cpuUsage = cpupercentage;
                        });
                    }

                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        });
        listenerThread.start();

        URL url = this.getClass().getResource("/MainView.fxml");
        FXMLLoader loader = new FXMLLoader(url);
        Parent root = loader.load();

        Controller controller = loader.<Controller>getController();

        controller.initData();

        primaryStage.setTitle("Task Manager");
        primaryStage.setScene(new Scene(root, 1210, 1000));
        primaryStage.setResizable(false);
        primaryStage.show();

    }


    private String readString(InputStream is) throws IOException {
        StringBuilder sb = new StringBuilder();
        int c = is.read();
        while(c != -1){
            if(c == '\n'){
                break;
            } else {
                sb.append((char) c);
                c = is.read();
            }
        }
        return sb.toString();
    }
}
