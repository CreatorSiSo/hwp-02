use dialoguer::Select;
use serialport::{ClearBuffer, DataBits, StopBits};
use std::{
    io::{stdin, stdout, Write},
    process::{self, Command},
    time::Duration,
};

fn main() {
    // Ask which port to use
    let ports = serialport::available_ports().expect("No ports found!");
    let selection = Select::new()
        .with_prompt("Which serial port should be used?")
        .items(
            &ports
                .iter()
                .map(|port_info| &port_info.port_name)
                .collect::<Vec<_>>(),
        )
        .default(0)
        .interact()
        .unwrap();
    println!();

    // Configure serial connection
    let mut port = serialport::new(&ports[selection].port_name, 115_200)
        .timeout(Duration::from_millis(10))
        .open()
        .expect("Failed to open port");
    port.set_timeout(Duration::from_secs(60)).unwrap();
    port.set_data_bits(DataBits::Eight).unwrap();
    port.set_stop_bits(StopBits::One).unwrap();

    let mut buffer: Vec<u8> = vec![0; 128];
    'outer: loop {
        // Send start signal to board and wait for response
        print!("Press enter to start sequence...");
        stdout().flush().unwrap();
        stdin().read_line(&mut String::new()).unwrap();
        port.write_all("Start".as_bytes()).unwrap();

        let mut mpv_process = None;

        // Receive and display messages from board
        loop {
            buffer.fill(0);
            port.clear(ClearBuffer::Input).unwrap();

            port.read(buffer.as_mut_slice()).expect("Found no data!");
            let received = String::from_utf8_lossy(&buffer);
            print!("{}", received);

            if received.starts_with("Starting") {
                mpv_process = Some(start_video_player());
            }

            if received.starts_with("End") || received.starts_with("Waiting") {
                println!();
                if let Some(mut child) = mpv_process {
                    child.kill().unwrap();
                }
                continue 'outer;
            }
        }
    }
}

fn start_video_player() -> process::Child {
    Command::new("mpv")
        .args(["./media/ricky.mp4", "--fullscreen"])
        .spawn()
        .unwrap()
}
