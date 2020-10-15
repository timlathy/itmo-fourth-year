package itmo.se.lab3.helpers

import org.openqa.selenium.WebDriver
import org.openqa.selenium.chrome.ChromeDriver
import org.openqa.selenium.firefox.FirefoxDriver
import java.lang.IllegalArgumentException

fun withDriver(type: String, quitDriver: Boolean = true, f: (WebDriver) -> Unit) {
    val driver = when (type) {
        "firefox" -> FirefoxDriver()
        "chrome" -> ChromeDriver()
        else -> throw IllegalArgumentException("Unknown driver type $type")
    }

    f(driver)

    if (quitDriver)
        driver.quit()
}
