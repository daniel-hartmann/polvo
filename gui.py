import gi
import threading

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gio, Gdk

from lib import Polvo, INSTRUMENTS


class PolvoApp(Gtk.ApplicationWindow):
    def __init__(self, app):
        super().__init__(
            application=app,
            title="Audio App",
            default_width=800,
            default_height=480,
        )

        # Create Polvo instance
        self.polvo = Polvo(status_callback=self.update_status)

        # Create main box
        self.main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.main_box.get_style_context().add_class("main")
        self.add(self.main_box)

        css_provider = Gtk.CssProvider()
        css_provider.load_from_path("style.css")

        # Add CSS provider to the application
        style_context = self.get_style_context()
        screen = Gdk.Screen.get_default()
        style_context.add_provider_for_screen(
            screen, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
        )

        # Create header bar
        self.create_header_bar()

        self.controls_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)
        self.controls_box.get_style_context().add_class("main")
        self.main_box.pack_start(self.controls_box, False, True, 0)

        # Button to start listening
        self.listen_button = self.create_button("Record")
        self.listen_button.get_style_context().add_class("record-button")
        self.listen_button.connect("clicked", self.on_listen_clicked)
        self.controls_box.pack_start(self.listen_button, False, True, 0)

        # Stop recording button
        self.stop_button = self.create_button("Stop")
        self.stop_button.get_style_context().add_class("stop-button")
        self.stop_button.set_sensitive(False)  # Initially disabled
        self.stop_button.connect("clicked", self.on_stop_clicked)
        self.controls_box.pack_start(self.stop_button, False, True, 0)

        # Play button
        self.play_button = self.create_button("Play")
        self.play_button.get_style_context().add_class("play-button")
        self.play_button.connect("clicked", self.on_play_clicked)
        self.controls_box.pack_start(self.play_button, False, True, 0)

        # Create a box for instrument buttons
        instrument_buttons_box = Gtk.Box(
            orientation=Gtk.Orientation.HORIZONTAL, spacing=10
        )
        instrument_buttons_box.get_style_context().add_class("instruments-box")

        self.main_box.pack_start(instrument_buttons_box, True, True, 0)

        # Create buttons for each instrument dynamically
        for instrument in INSTRUMENTS:
            button = self.create_image_button(f"images/{instrument}.png", instrument)
            button.get_style_context().add_class("instrument-button")
            button.connect("clicked", self.on_instrument_clicked, instrument)
            instrument_buttons_box.pack_start(button, False, True, 0)

        # Status bar
        self.status_bar = Gtk.Statusbar()
        self.update_status("Ready")
        self.main_box.pack_end(self.status_bar, False, True, 0)

    def create_header_bar(self):
        header = Gtk.HeaderBar()
        header.set_show_close_button(True)
        header.props.title = "Audio App"
        self.set_titlebar(header)

    def update_status(self, text):
        self.status_bar.pop(0)  # Remove previous message
        context_id = self.status_bar.get_context_id("Status")
        self.status_bar.push(context_id, text)

    def create_button(self, label):
        button = Gtk.Button(label=label)
        button.set_size_request(200, 200)
        return button

    def create_stop_button(self, label):
        button = Gtk.Button(label=label)
        button.set_size_request(80, 80)
        return button

    def create_image_button(self, image_file, label_text):
        button = Gtk.Button()

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)

        image = Gtk.Image.new_from_file(image_file)
        image.set_pixel_size(40)  # Set the size of the image

        label = Gtk.Label(label_text.upper())

        # Pack the image and label widgets into the box
        box.pack_start(image, True, True, 0)  # Allow image to expand
        box.pack_start(label, False, False, 0)  # Keep label size fixed

        # Set the box as the content of the button
        button.add(box)

        # Show image and label always
        button.set_always_show_image(True)

        button.set_size_request(60, 60)  # Set the size of the button
        return button

    def on_listen_clicked(self, widget):
        # Start listening in a separate thread
        threading.Thread(target=self.polvo.listen).start()
        self.listen_button.set_sensitive(False)  # Disable the record button
        self.stop_button.set_sensitive(True)  # Enable the stop button

    def on_stop_clicked(self, widget):
        # Stop the recording process
        self.polvo.stop()
        self.stop_button.set_sensitive(False)  # Disable the stop button
        self.listen_button.set_sensitive(True)  # Enable the record button

    def on_play_clicked(self, widget):
        # Play last recording
        self.polvo.play_last_recording()
        self.stop_button.set_sensitive(False)  # Disable the stop button
        self.listen_button.set_sensitive(True)  # Enable the record button

    def on_instrument_clicked(self, widget, instrument):
        # Start playing the selected instrument in a separate thread
        threading.Thread(target=self.polvo.play_instrument, args=(instrument,)).start()


class PolvoApplication(Gtk.Application):
    def __init__(self):
        super().__init__(
            application_id="org.example.polvoapp",
            flags=Gio.ApplicationFlags.FLAGS_NONE,
        )

    def do_activate(self):
        win = PolvoApp(self)
        win.set_decorated(False)
        win.fullscreen()
        win.show_all()


def init():
    app = PolvoApplication()
    app.run(None)
