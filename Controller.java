import javafx.collections.ListChangeListener;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class Controller {
    public VBox processesVbox;
    public HBox cpuHistoryHbox;
    public Label cpuUsageLabel;

    private XYChart.Series<Integer, Float> series;
    private int currentTime;
    private LineChart<Integer, Float> linechart;

    public void initData(){
        ClientApplication.processData.addListener(new ListChangeListener<String>() {
            @Override
            public void onChanged(Change<? extends String> c) {
                update();
            }
        });


        //Defining X axis
        NumberAxis xAxis = new NumberAxis(0, 60, 1);
        xAxis.setLabel("Seconds");

        //Defining y axis
        NumberAxis yAxis = new NumberAxis(0, 100, 10);
        yAxis.setLabel("CPU usage");

        //Creating the line chart
        linechart = new LineChart(xAxis, yAxis);
        linechart.setPrefSize(1200, 600);

        series = new XYChart.Series<>();

        linechart.getData().add(series);

        cpuHistoryHbox.getChildren().addAll(linechart);
    }

    private void update(){
        cpuUsageLabel.setText(ClientApplication.cpuUsage + " %");
        ClientApplication.processData.forEach(s -> processesVbox.getChildren().add(createRow(s)));
        if(ClientApplication.time != currentTime){
            processesVbox.getChildren().clear();
            System.out.println("time : " + ClientApplication.time);
            if(ClientApplication.time != -1 && ClientApplication.time <= 60){
                series.getData().add(new XYChart.Data<>(ClientApplication.time, (float) ClientApplication.cpuUsage ));
            }
            else if(ClientApplication.time != -1){
                //shift the series and add the current time as the 60th second
                series.getData().remove(0);
                series.getData().stream().forEach(d -> d.setXValue(d.getXValue() - 1));
                series.getData().add(new XYChart.Data<>(60, (float) ClientApplication.cpuUsage ));
            }

            currentTime = ClientApplication.time;
        }

    }

    private HBox createRow(String row){
        String[] vals = row.split(",");

        HBox rowHbox = new HBox(10);
        Label nameLabel = new Label(vals[0]);
        nameLabel.setPrefWidth(200);
        Label cpuUsage = new Label(vals[1] + " %");
        cpuUsage.setPrefWidth(100);
        Label ramUsage = new Label(vals[2] + " MB");
        ramUsage.setPrefWidth(100);
        Label virtualMemory = new Label(vals[3] + " MB");
        virtualMemory.setPrefWidth(100);
        rowHbox.getChildren().addAll(nameLabel, cpuUsage, ramUsage, virtualMemory);
        return rowHbox;
    }
}
