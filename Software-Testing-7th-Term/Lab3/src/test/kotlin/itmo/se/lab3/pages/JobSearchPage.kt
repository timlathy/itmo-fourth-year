package itmo.se.lab3.pages

import itmo.se.lab3.pages.elements.CheckboxElement
import itmo.se.lab3.pages.elements.ListingElement
import org.openqa.selenium.*
import org.openqa.selenium.support.*
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.Select
import org.openqa.selenium.support.ui.WebDriverWait

class JobSearchPage(private val driver: WebDriver) {
    @FindBy(xpath = "//form[@name=\"search_job_form\"]/following-sibling::*/a[text()=\"Искать\"]")
    lateinit var searchButton: WebElement

    @FindBy(xpath = "//label[text()=\"Тип работы:\"]/following-sibling::span")
    lateinit var categoryDropdownButton: WebElement

    val sortSelector
        get() = Select(driver.findElement(By.xpath("//span[text()=\"Сортировка: \"]/following-sibling::select[not(@class=\"right\")]")))

    init {
        driver["https://advego.com/job/find/"]
        PageFactory.initElements(driver, this)
    }

    fun getCategories(): List<CheckboxElement> {
        categoryDropdownButton.click()

        val checkboxLocator =
            By.xpath("//label[text()=\"Тип работы:\"]/following-sibling::div//div[contains(@class, \"ui-dropdownchecklist-item\")]")
        WebDriverWait(driver, 2).until(visibilityOfElementLocated(checkboxLocator))
        return driver.findElements(checkboxLocator).map { CheckboxElement(it) }
    }

    fun waitForItems() {
        val itemsLocator = By.xpath("//div[@class=\"list\"]/*")
        WebDriverWait(driver, 8).until(presenceOfElementLocated(itemsLocator))
    }

    fun listItems(): List<ListingElement> = ListingElement.findAll(driver)
}
