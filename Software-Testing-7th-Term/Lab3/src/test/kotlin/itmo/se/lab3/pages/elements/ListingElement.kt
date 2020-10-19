package itmo.se.lab3.pages.elements

import itmo.se.lab3.helpers.extract
import org.openqa.selenium.*
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.WebDriverWait

class ListingElement(private val driver: WebDriver, private val container: WebElement) {
    companion object {
        fun findAll(driver: WebDriver): List<ListingElement> =
            driver.findElements(By.xpath("//div[@class=\"list_item\" and *//@class = \"order-title\"]"))
                .map { ListingElement(driver, it) }
    }

    val title: String = container.findElement(By.xpath(".//*[@class=\"order-title\"]")).text
    val description: String = container.findElement(By.xpath(".//div[contains(@class, \"job_desc\")]")).text
    val price =
        container.findElement(By.xpath("//div[@class=\"price\"]")).text.extract(Regex("([.\\d]+) руб.")).toDouble()

    fun getCategory(): String =
        container.findElement(By.xpath(".//span[@class=\"order-type\"]")).text

    fun getKeywords(): List<String> =
        container.findElement(By.xpath(".//div[starts-with(@id, \"tab-keywords\")]")).text.split(Regex(" \\d+, "))

    fun addToCart() {
        val addButton = container.findElement(By.xpath(".//a[text()=\"Отложить в корзину\"]"))
        addButton.click()
        val statusLocator = By.xpath(".//*[contains(text(), \"добавлена в\")]")
        WebDriverWait(driver, 8).until(presenceOfNestedElementLocatedBy(container, statusLocator))
    }

    fun removeFromCart() {
        val removeButton = container.findElement(By.xpath(".//a[text()=\"Удалить из корзины\"]"))
        removeButton.click()
        val statusLocator = By.xpath(".//*[contains(text(), \"удалена из корзины\")]")
        WebDriverWait(driver, 8).until(presenceOfNestedElementLocatedBy(container, statusLocator))
    }

    fun removeOwnListing() {
        val removeListingButton = container.findElement(By.xpath(".//a[text()=\"Удалить\"]"))
        removeListingButton.click()
        val confirmButton = container.findElement(By.xpath(".//a[text()=\"Удалить статью\"]"))
        confirmButton.click()
        val statusLocator = By.xpath(".//*[contains(text(), \"Статья удалена\")]")
        WebDriverWait(driver, 8).until(presenceOfNestedElementLocatedBy(container, statusLocator))
    }
}