use std::{io::Write, time::Duration};

use dialoguer::Select;
use serialport::{ClearBuffer, DataBits, StopBits};

fn main() {
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

    let mut port = serialport::new(&ports[selection].port_name, 115_200)
        .timeout(Duration::from_millis(10))
        .open()
        .expect("Failed to open port");
    port.set_timeout(Duration::from_secs(60)).unwrap();
    port.set_data_bits(DataBits::Eight).unwrap();
    port.set_stop_bits(StopBits::One).unwrap();

    let mut buf: Vec<u8> = vec![0; 128];
    'repl: loop {
        print!("Press enter to start sequence...");
        std::io::stdout().flush().unwrap();
        std::io::stdin().read_line(&mut String::new()).unwrap();
        port.write_all("Start".as_bytes()).unwrap();

        let mut vlc = None;

        loop {
            buf.fill(0);
            port.clear(ClearBuffer::Input).unwrap();

            port.read(buf.as_mut_slice()).expect("Found no data!");
            let received = String::from_utf8_lossy(&buf);
            print!("{}", received);

            if received.starts_with("Starting") {
                vlc = Some(
                    std::process::Command::new("mpv")
                        .args(["./media/ricky.mp4", "--fullscreen"])
                        .spawn()
                        .unwrap(),
                );
            }

            if received.starts_with("End") || received.starts_with("Waiting") {
                println!();
                vlc.map(|mut process| process.kill().unwrap());
                continue 'repl;
            }
        }
    }
}
